package db;

import com.amazonaws.services.secretsmanager.AWSSecretsManager;
import com.amazonaws.services.secretsmanager.AWSSecretsManagerClientBuilder;
import com.amazonaws.services.secretsmanager.model.GetSecretValueRequest;
import com.amazonaws.services.secretsmanager.model.GetSecretValueResult;
import com.google.gson.Gson;
import com.google.gson.JsonObject;
import lombok.Getter;

import java.util.Base64;

@Getter
public class DBSecret {
    private final String dbName;
    private final String dbHostName;
    private final String dbPort;
    private final String dbUsername;
    private final String dbPassword;

    public DBSecret(final String region, final String secretArn) {
        AWSSecretsManager secretsManager = AWSSecretsManagerClientBuilder.standard().withRegion(region).build();
        String secret;
        GetSecretValueRequest getSecretValueRequest = new GetSecretValueRequest().withSecretId(secretArn);
        GetSecretValueResult getSecretValueResult = secretsManager.getSecretValue(getSecretValueRequest);

        if (getSecretValueResult.getSecretString() != null)
            secret = getSecretValueResult.getSecretString();
        else
            secret = new String(Base64.getDecoder().decode(getSecretValueResult.getSecretBinary()).array());

        JsonObject secretJson = new Gson().fromJson(secret, JsonObject.class);


        dbName = secretJson.get("dbname").getAsString();
        dbHostName = secretJson.get("host").getAsString();
        dbPort = secretJson.get("port").getAsString();
        dbUsername = secretJson.get("username").getAsString();
        dbPassword = secretJson.get("password").getAsString();
    }
}
