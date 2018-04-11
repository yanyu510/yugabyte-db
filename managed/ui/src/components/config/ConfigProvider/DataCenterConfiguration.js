// Copyright (c) YugaByte, Inc.

import React, { Component } from 'react';
import { withRouter } from 'react-router';
import { AzureProviderConfigurationContainer, DockerProviderConfigurationContainer,
         OnPremConfigurationContainer, ProviderConfigurationContainer, StorageConfigurationContainer } from '../../config';
import {Tab} from 'react-bootstrap';
import { YBTabsPanel } from '../../panels';
import './providerConfig.scss';
import awsLogo from './images/aws.png';
import azureLogo from './images/azure.png';
import dockerLogo from './images/docker.png';
import gcpLogo from './images/gcp.png';

class DataCenterConfiguration extends Component {
  configProviderSelect = item => {
    const currentLocation = this.props.location;
    currentLocation.query = { provider: item };
    this.props.router.push(currentLocation);
  };

  render() {
    const onPremiseTabContent = (
      <div className="on-premise">
        <i className="fa fa-server" />
        On-Premises<br/>Datacenters
      </div>
    );

    return (
      <div>
        <h2 className="content-title">Cloud Provider Configuration</h2>
        <YBTabsPanel defaultTab="cloud" activeTab={this.props.params.tab} routePrefix="/config/" id="config-tab-panel">
          <Tab eventKey="cloud" title="Cloud" key="cloud-config">
            <YBTabsPanel defaultTab="aws" activeTab={this.props.params.section} id="cloud-config-tab-panel" className="config-tabs" routePrefix="/config/cloud/">
              <Tab eventKey="aws" title={<img src={awsLogo} alt="AWS" className="aws-logo" />} key="aws-tab" unmountOnExit={true}>
                <ProviderConfigurationContainer providerType="aws" />
              </Tab>
              <Tab eventKey="gcp" title={<img src={gcpLogo} alt="GCP" className="gcp-logo" />} key="gcp-tab" unmountOnExit={true}>
                <ProviderConfigurationContainer providerType="gcp" />
              </Tab>
              <Tab eventKey="azure" title={<img src={azureLogo} alt="Azure" className="azure-logo" />} key="azure-tab" unmountOnExit={true}>
                <AzureProviderConfigurationContainer />
              </Tab>
              <Tab eventKey="docker" title={<img src={dockerLogo} alt="Docker" className="docker-logo" />} key="docker-tab" unmountOnExit={true}>
                <DockerProviderConfigurationContainer />
              </Tab>
              <Tab eventKey="onprem" title={onPremiseTabContent} key="onprem-tab" unmountOnExit={true}>
                <OnPremConfigurationContainer params={this.props.params} />
              </Tab>
            </YBTabsPanel>
          </Tab>
          <Tab eventKey="storage" title="Storage" key="storage-config">
            <YBTabsPanel defaultTab="aws" activeTab={this.props.params.section} id="storage-config-tab-panel" className="config-tabs" routePrefix="/config/storage/">
              <Tab eventKey="aws" title={<img src={awsLogo} alt="AWS" className="aws-logo" />} key="aws-tab" unmountOnExit={true}>
                <StorageConfigurationContainer />
              </Tab>
            </YBTabsPanel>
          </Tab>
        </YBTabsPanel>
      </div>
    );
  }
}
export default withRouter(DataCenterConfiguration);
