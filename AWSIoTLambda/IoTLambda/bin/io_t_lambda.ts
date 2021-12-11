#!/usr/bin/env node
import 'source-map-support/register';
import * as cdk from 'aws-cdk-lib';
import {IoTLambdaStack, IoTLambdaStackProps} from '../lib/io_t_lambda-stack';
import { account, dbUsername } from "../aws_account";
import {StackProps} from "aws-cdk-lib";

const stackProps: StackProps = {
    env: { region: account.region, account: `${account.account_id}` },
}

const props: IoTLambdaStackProps = {
    props: stackProps,
    account: `${account.account_id}`,
    region: account.region,
    dbPort: "3306",
    dbUsername: dbUsername
}

const app = new cdk.App();
new IoTLambdaStack(app, `IoTLambdaStack-${account.region}`, props);
