import {Stack, StackProps} from 'aws-cdk-lib';
import {Construct} from 'constructs';
import * as lambda from 'aws-cdk-lib/aws-lambda';
import * as path from "path";
import {Effect, PolicyStatement} from "aws-cdk-lib/aws-iam";
import {Queue} from "aws-cdk-lib/aws-sqs";

// import * as sqs from 'aws-cdk-lib/aws-sqs';

export class IoTLambdaStack extends Stack {
  constructor(scope: Construct, id: string, props?: StackProps) {
    super(scope, id, props);

    // The code that defines your stack goes here
    const lambdaFunction = new lambda.Function(this, 'IoTLambdaFunction', {
      runtime: lambda.Runtime.JAVA_11,
      handler: 'IoTLambdaHandler.AWSIoTLambdaHandler::handleRequest',
      code: lambda.Code.fromAsset(path.join('../IoTLambdaHandler/target', 'classes'))
    });

    // SQS
    const sqs = new Queue(this, 'IoTSQS', {});
    sqs.grant(lambdaFunction, '*');
    sqs.grantConsumeMessages(lambdaFunction);

    // Pull messages from SQS Policy
    // const policyStatement = new PolicyStatement({
    //   actions: ["sqs.DeleteMessage", "sqs.ReceiveMessage", "sqs.ChangeMessageVisibility"],
    //   effect: Effect.ALLOW,
    //   resources: [sqs.queueArn]
    // })
    // lambdaFunction.addToRolePolicy(policyStatement);

    // example resource
    // const queue = new sqs.Queue(this, 'IoTLambdaQueue', {
    //   visibilityTimeout: cdk.Duration.seconds(300)
    // });
  }
}
