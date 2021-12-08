package handler;

import com.amazonaws.services.lambda.runtime.Context;
import com.amazonaws.services.lambda.runtime.LambdaLogger;
import com.amazonaws.services.lambda.runtime.RequestHandler;
import com.amazonaws.services.lambda.runtime.events.SQSEvent;
import com.amazonaws.services.lambda.runtime.events.SQSEvent.SQSMessage;

import java.util.TreeSet;

// Handler value: handler.Handler
public class Handler implements RequestHandler<SQSEvent, Void>{
  @Override
  public Void handleRequest(SQSEvent sqsEvent, Context context) {
    LambdaLogger logger = context.getLogger();
    TreeSet<Integer> processedMessagesHashCodes = new TreeSet<>();

    for (SQSMessage message: sqsEvent.getRecords()) {

      if (processedMessagesHashCodes.contains(message.hashCode()))
        continue;

      processedMessagesHashCodes.add(message.hashCode());

      logger.log(message.getBody());
    }
    return null;
  }
}