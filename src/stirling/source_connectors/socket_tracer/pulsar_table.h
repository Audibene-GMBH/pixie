/*
 * Copyright 2018- The Pixie Authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "src/stirling/core/output.h"
#include "src/stirling/core/types.h"

namespace px {
namespace stirling {
constexpr DataElement kPulsarElements[] = {
    {"cmd", "The name of the command.", types::DataType::STRING, types::SemanticType::ST_NONE,
     types::PatternType::GENERAL},
    {"topic", "The topic in which the operation.", types::DataType::STRING,
     types::SemanticType::ST_NONE, types::PatternType::GENERAL},
};

constexpr DataTableSchema kPulsarTable("pulsar_events.beta", "Pulsar messages.", kPulsarElements);
DEFINE_PRINT_TABLE(Pulsar)

namespace pulsar_idx {
constexpr int kCMD = kPulsarTable.ColIndex("cmd");
}  // namespace pulsar_idx

}  // namespace stirling
}  // namespace px
