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

package testingutils

import (
	"time"
)

// MockContext provides a mock context for testing.
type MockContext struct{}

// Deadline returns the deadline for the mock context.
func (ctx MockContext) Deadline() (deadline time.Time, ok bool) {
	return deadline, ok
}

// Done returns the done channel for the mock context.
func (ctx MockContext) Done() <-chan struct{} {
	ch := make(chan struct{})
	return ch
}

// Err returns the error for the mock context.
func (ctx MockContext) Err() error {
	return nil
}

// Value returns the value for the mock context.
func (ctx MockContext) Value(key interface{}) interface{} {
	return nil
}
