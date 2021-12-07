package handler;

import com.amazonaws.services.lambda.runtime.Context;
import com.amazonaws.services.lambda.runtime.LambdaLogger;
import com.amazonaws.services.lambda.runtime.RequestHandler;
import com.amazonaws.services.lambda.runtime.events.SQSEvent;
import com.amazonaws.services.lambda.runtime.events.SQSEvent.SQSMessage;

// Handler value: handler.Handler
public class Handler implements RequestHandler<SQSEvent, Void>{

  @Override
  public Void handleRequest(SQSEvent sqsEvent, Context context) {
    LambdaLogger logger = context.getLogger();
    for (SQSMessage message: sqsEvent.getRecords()) {
      logger.log(message.getBody());
    }
    return null;
  }
}