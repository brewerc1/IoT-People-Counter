#!/bin/sh

FILE=aws_account.txt

if [ ! -f "$FILE" ]; then
  echo "aws_account.txt is needed"
  echo "Please, run generate_aws_account_file.sh to create it."
fi

npx cdk synth
#
# Bootstrapping
input="aws_account.txt"
while IFS= read -r line
do
  cdk bootstrap aws://"$line"
done < "$input"
#
##
npx cdk deploy
