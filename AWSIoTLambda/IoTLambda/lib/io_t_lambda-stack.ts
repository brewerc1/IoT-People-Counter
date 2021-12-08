import {Stack, StackProps} from 'aws-cdk-lib';
import {Construct} from 'constructs';
import * as lambda from 'aws-cdk-lib/aws-lambda';
import * as path from "path";
import { Queue } from "aws-cdk-lib/aws-sqs";

export class IoTLambdaStack extends Stack {
  constructor(scope: Construct, id: string, props?: StackProps) {
    super(scope, `${id}-1`, props);

    // The code that defines your stack goes here
    const lambdaFunction = new lambda.Function(this, 'IoTLambdaFunction-1', {
      runtime: lambda.Runtime.JAVA_11,
      handler: 'handler.Handler::handleRequest',
      code: lambda.Code.fromAsset(path.join('../IoTLambdaHandler/build/distributions/IoTLambdaHandler.zip'))
    });

    // SQS
    const sqs = new Queue(this, 'IoTSQS-1', {});
    sqs.grant(lambdaFunction, '*');
    sqs.grantConsumeMessages(lambdaFunction);
  }
}
