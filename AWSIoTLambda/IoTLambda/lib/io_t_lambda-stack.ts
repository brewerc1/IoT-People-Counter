import { Stack, StackProps } from 'aws-cdk-lib';
import { Construct } from 'constructs';
import * as lambda from 'aws-cdk-lib/aws-lambda';
import * as path from "path";
// import * as sqs from 'aws-cdk-lib/aws-sqs';

export class IoTLambdaStack extends Stack {
  constructor(scope: Construct, id: string, props?: StackProps) {
    super(scope, id, props);

    // The code that defines your stack goes here
    const lambdaFunction = new lambda.Function(this, 'IoTLambdaFunction', {
      runtime: lambda.Runtime.JAVA_11,
      handler: '',
      code: lambda.Code.fromAsset(path.join(__dirname, 'lambda'))
    });

    // example resource
    // const queue = new sqs.Queue(this, 'IoTLambdaQueue', {
    //   visibilityTimeout: cdk.Duration.seconds(300)
    // });
  }
}
