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
import {
  Theme, makeStyles,
} from '@material-ui/core';
import { createStyles } from '@material-ui/styles';
import { ClusterContext } from 'common/cluster-context';
import { useListClusters } from '@pixie-labs/api-react';
import { StatusCell, Select } from '@pixie-labs/components';
import { GQLClusterStatus as ClusterStatus } from '@pixie-labs/api';
import { clusterStatusGroup } from 'containers/admin/utils';

const useStyles = makeStyles(({ spacing, palette }: Theme) => createStyles({
  label: {
    marginRight: spacing(0.5),
    justifyContent: 'center',
    display: 'flex',
    alignItems: 'center',
    color: palette.common.white,
    fontWeight: 800,
  },
  container: {
    display: 'flex',
    justifyContent: 'center',
    flexDirection: 'row',
  },
}));

const ClusterSelector: React.FC = () => {
  const classes = useStyles();

  const [clusters, loading, error] = useListClusters();
  const { selectedCluster, setCluster } = React.useContext(ClusterContext);

  const clusterName = React.useMemo(
    () => clusters?.find((c) => c.id === selectedCluster)?.prettyClusterName || 'unknown cluster',
    [clusters, selectedCluster]);

  const clusterNameIdMap = React.useMemo(
    () => new Map<string, string>(clusters?.map((c) => [c.prettyClusterName, c.id])),
    [clusters]);

  const getListItems = React.useCallback(async (input: string) => (
    clusters
      ?.filter((c) => c.status !== ClusterStatus.CS_DISCONNECTED && c.prettyClusterName.includes(input))
      .map((c) => ({
        value: c.prettyClusterName,
        icon: <StatusCell statusGroup={clusterStatusGroup(c.status)} />,
      }))
  ), [clusters]);

  const onSelect = React.useCallback(
    (input: string) => setCluster(clusterNameIdMap.get(input)),
    [clusterNameIdMap, setCluster]);

  if (loading || !clusters || error) return (<></>);

  return (
    <div className={classes.container}>
      <div className={classes.label}>Cluster:</div>
      <Select
        value={clusterName}
        getListItems={getListItems}
        onSelect={onSelect}
        requireCompletion
      />
    </div>
  );
};

export default ClusterSelector;
