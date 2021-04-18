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

import * as React from 'react';

import SvgIcon, { SvgIconProps } from '@material-ui/core/SvgIcon';

export const UnexpandedIcon = (props: SvgIconProps) => (
  <SvgIcon
    {...props}
    width='24'
    height='24'
    viewBox='0 0 24 24'
    fill='none'
    xmlns='http://www.w3.org/2000/svg'
  >
    <g opacity='0.45'>
      <path
        d='M10 6L8.59 7.41L13.17 12L8.59 16.59L10 18L16 12L10 6Z'
        fill='white'
      />
    </g>
  </SvgIcon>
);
