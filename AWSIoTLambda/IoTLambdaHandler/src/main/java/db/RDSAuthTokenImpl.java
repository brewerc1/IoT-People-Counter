package db;

import com.amazonaws.auth.DefaultAWSCredentialsProviderChain;
import com.amazonaws.services.rds.auth.GetIamAuthTokenRequest;
import com.amazonaws.services.rds.auth.RdsIamAuthTokenGenerator;

// AWS: Credentials of the IAM User
// We obtain the AWS credentials from default profile
// https://docs.aws.amazon.com/sdk-for-java/v1/developer-guide/credentials.html

public class RDSAuthTokenImpl implements RDSAuthToken {
    private final String region;
    private final String hostName;
    private final int port;
    private final String username;

    public RDSAuthTokenImpl(final String region, final String hostName, final int port, final String username) {
        this.region = region;
        this.hostName = hostName;
        this.port = port;
        this.username = username;
    }

    @Override
    public String generateToken() {
        RdsIamAuthTokenGenerator generator = RdsIamAuthTokenGenerator.builder()
                .credentials(new DefaultAWSCredentialsProviderChain())
                .region(region)
                .build();

        return generator.getAuthToken(
                GetIamAuthTokenRequest.builder()
                        .hostname(hostName)
                        .port(port)
                        .userName(username)
                        .build()
        );
    }
}
