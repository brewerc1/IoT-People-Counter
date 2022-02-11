import logging
import json
import os
import boto3

logger = logging.getLogger()
logger.setLevel(logging.INFO)
dynamodb = boto3.resource('dynamodb').Table(os.environ.get("TABLE"))

def lambda_handler(event, context):
    logger.info('Recieved event from SQS, starting processing')
    records = event['Records']
    for record in records:
        attributes = record['attributes']
        payload = json.loads(record['body'])
        logger.info('Processing record: ' + json.dumps(payload))

        data = {
            'PK': f'DEVICE#{payload["device"]}',
            'SK': attributes['SentTimestamp'],
            'total': payload['total']
        }

        logger.info('Inserting data: ' + json.dumps(data))

        try:
            dynamodb.put_item(Item=data)
            logger.info('Inserted data: ' + json.dumps(data))
        except Exception as exception:
            logger.error('Error inserting data: ' + str(exception))
        
        logger.info('Finished processing record: ' + json.dumps(payload))

    logger.info('Finished processing SQS event')