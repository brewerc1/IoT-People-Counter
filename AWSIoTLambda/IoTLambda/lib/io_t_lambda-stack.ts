import {Duration, Stack, StackProps} from 'aws-cdk-lib';
import { Construct } from 'constructs';
import * as lambda from 'aws-cdk-lib/aws-lambda';
import * as path from "path";
import { Queue } from "aws-cdk-lib/aws-sqs";
import * as ec2 from "aws-cdk-lib/aws-ec2";
import * as rds from "aws-cdk-lib/aws-rds";
import { Port } from "aws-cdk-lib/aws-ec2";

export interface IoTLambdaStackProps {
  props: StackProps;
  account: string;
  region: string;
  dbPort: string;
  dbUsername: string;
}


export class IoTLambdaStack extends Stack {
  constructor(scope: Construct, id: string, props: IoTLambdaStackProps ) {
    super(scope, id, props?.props);

    const TIME_OUT_IN_MINUTES = 3;

    // SQS
    const sqs = new Queue(this, `IoTSqs-${props.region}`, {
      visibilityTimeout: Duration.minutes(TIME_OUT_IN_MINUTES)
    });

    const vpc = new ec2.Vpc(this, `IoTPrivateVPC-${props.region}`, {
      subnetConfiguration: [
        {
          name: `private-subnet-0-${props.region}`,
          subnetType: ec2.SubnetType.PUBLIC,
          cidrMask: 24,
        },
        {
          name: `private-subnet-1-${props.region}`,
          subnetType: ec2.SubnetType.PRIVATE_WITH_NAT,
          cidrMask: 24,
        },
        {
          name: `private-subnet-2-${props.region}`,
          subnetType: ec2.SubnetType.PRIVATE_ISOLATED,
          cidrMask: 24,
        }
      ]
    });

    const cluster = new rds.DatabaseCluster(this, `IoTDb-${props.region}`, {
      engine: rds.DatabaseClusterEngine.AURORA_MYSQL,
      credentials: rds.Credentials.fromGeneratedSecret(props.dbUsername),
      instanceProps: {
        instanceType: ec2.InstanceType.of(ec2.InstanceClass.BURSTABLE2, ec2.InstanceSize.SMALL),
        // vpcSubnets: {
        //   subnetType: ec2.SubnetType.PRIVATE_ISOLATED
        // },
        vpc: vpc
      },
      defaultDatabaseName: 'IoTDb',
      port: Number(props.dbPort)
    });

    // The code that defines your stack goes here
    const lambdaFunction = new lambda.Function(this, `IoTLambdaFunction-${props.region}`, {
      runtime: lambda.Runtime.JAVA_11,
      handler: 'handler.Handler::handleRequest',
      code: lambda.Code.fromAsset(path.join('../IoTLambdaHandler/build/distributions/IoTLambdaHandler.zip')),
      environment: {
        'region': props.region,
        'dbInstanceSecretArn': cluster.secret?.secretArn || "default"
      },
      timeout: Duration.minutes(TIME_OUT_IN_MINUTES),
      memorySize: 256,
      vpc
    });

    cluster.secret?.grantRead(lambdaFunction);
    cluster.connections.allowFrom(lambdaFunction, Port.tcp(Number(props.dbPort)));

    sqs.grant(lambdaFunction, '*');
    sqs.grantConsumeMessages(lambdaFunction);

  }
}
