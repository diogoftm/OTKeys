#!/bin/bash
CA_NAME_=root_CA

openssl req -x509 -nodes -newkey rsa:4096 -sha256 -days 3650 -keyout $CA_NAME_.key -out $CA_NAME_.pem -subj "/CN=$CA_NAME_"

for CN_ in 127.0.0.1; do
  openssl req -new -newkey rsa:4096 -nodes -keyout $CN_.key -out $CN_.csr -subj "/CN=$CN_" -addext "subjectAltName=IP:$CN_" || openssl req -new -newkey rsa:4096 -nodes -keyout $CN_.key -out $CN_.csr -subj "/CN=$CN_" -addext "subjectAltName=DNS:$CN_"
  openssl x509 -req -in $CN_.csr -CA $CA_NAME_.pem -CAkey $CA_NAME_.key -CAcreateserial -out $CN_.pem -days 3650 -sha256
  openssl x509 -in $CN_.pem -text -noout  # openssl x509 -in file.pem -enddate -noout
done

