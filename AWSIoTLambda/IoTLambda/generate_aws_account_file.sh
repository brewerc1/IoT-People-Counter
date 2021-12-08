#!/bin/sh

echo "AWS Account Number (14 digit):"
read -r account

echo "AWS Region:"
read -r region

echo "$account/$region" > "aws_account.txt"
