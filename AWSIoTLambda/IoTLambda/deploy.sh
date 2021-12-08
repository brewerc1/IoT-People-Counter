#!/bin/sh

FILE=aws_account.txt

if [ ! -f "$FILE" ]; then
  echo "aws_account.txt is needed"
  echo "Please, run generate_aws_account_file.sh to create it."
else
  npx cdk synth
  #
  # Bootstrapping
  while IFS= read -r line
  do
    cdk bootstrap aws://"$line"
  done < "$FILE"
  #
  ##
  npx cdk deploy
fi
