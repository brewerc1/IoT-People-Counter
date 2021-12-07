package IoTLambdaHandler;

import com.amazonaws.services.lambda.runtime.Context;
import com.amazonaws.services.lambda.runtime.LambdaLogger;
import com.amazonaws.services.lambda.runtime.RequestHandler;

public class AWSIoTLambdaHandler implements RequestHandler<Void, Void> {
    @Override
    public Void handleRequest(Void unused, Context context) {
        LambdaLogger logger = context.getLogger();
        logger.log("Hello world");
        return null;
    }
}
