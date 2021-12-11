#!/bin/sh

echo "AWS Account Number (14 digit):"
read -r account

echo "AWS Region:"
read -r region

echo "AWS RDS admin username:"
read -r dbUsername

echo "$account/$region" > "aws_account.txt"

echo "export const account = {account_id: $account, region: '$region'};" > "aws_account.ts"
echo "export const dbUsername = '$dbUsername';" >> "aws_account.ts"
