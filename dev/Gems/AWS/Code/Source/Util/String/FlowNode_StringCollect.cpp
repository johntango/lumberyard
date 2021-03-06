/*
* All or portions of this file Copyright (c) Amazon.com, Inc. or its affiliates or
* its licensors.
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/
#include <AWS_precompiled.h>
#include <Util/String/FlowNode_StringCollect.h>
// The AWS Native SDK AWSAllocator triggers a warning due to accessing members of std::allocator directly.
// AWSAllocator.h(70): warning C4996: 'std::allocator<T>::pointer': warning STL4010: Various members of std::allocator are deprecated in C++17.
// Use std::allocator_traits instead of accessing these members directly.
// You can define _SILENCE_CXX17_OLD_ALLOCATOR_MEMBERS_DEPRECATION_WARNING or _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS to acknowledge that you have received this warning.

#include <AzCore/PlatformDef.h>
AZ_PUSH_DISABLE_WARNING(4251 4996, "-Wunknown-warning-option")
#include <aws/core/utils/StringUtils.h>
AZ_POP_DISABLE_WARNING

namespace LmbrAWS
{
    static const char* CLASS_TAG = "String:Collect";

    void FlowNode_StringCollect::GetConfiguration(SFlowNodeConfig& configuration)
    {
        static const SInputPortConfig inputPortConfiguration[] =
        {
            InputPortConfig_Void("Activate", "Collect the strings and activate the output port"),
            InputPortConfig<string>("Input", "Input each string to join to this port"),
            InputPortConfig<string>("JoinString", "String to use in between all incoming strings, such as , or \\n"),
            { 0 }
        };

        static const SOutputPortConfig outputPortConfiguration[] =
        {
            OutputPortConfig<string>("CollectedString", "The final collected string"),
            { 0 }
        };

        configuration.pInputPorts = inputPortConfiguration;
        configuration.pOutputPorts = outputPortConfiguration;
        configuration.sDescription = _HELP("String collect will collect all strings input into the node. When Activate is called, a single string will be output which is all input strings joined by the join character");
        configuration.SetCategory(EFLN_APPROVED);
    }

    void FlowNode_StringCollect::ProcessEvent(EFlowEvent event, SActivationInfo* pActInfo)
    {
        if (event == eFE_Activate && pActInfo->pInputPorts[1].IsUserFlagSet())
        {
            Aws::String inputString = GetPortString(pActInfo, 1).c_str();
            m_strings.push_back(inputString);
        }
        if (event == eFE_Activate && pActInfo->pInputPorts[0].IsUserFlagSet())
        {
            Aws::String joinStr = GetPortString(pActInfo, 2).c_str();

            Aws::String outputString;
            for (auto& stringToJoin : m_strings)
            {
                outputString.append(stringToJoin).append(joinStr);
            }
            //Cut off the last join str
            outputString = outputString.substr(0, outputString.length() - joinStr.length());
            SFlowAddress addr(pActInfo->myID, 0, true);
            pActInfo->pGraph->ActivatePort(addr, string(outputString.c_str()));
            m_strings = Aws::Vector<Aws::String>();
        }
    }

    REGISTER_CLASS_TAG_AND_FLOW_NODE(CLASS_TAG, FlowNode_StringCollect);
} // namespace AWS





