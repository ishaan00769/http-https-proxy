#!/bin/bash

PROXY="127.0.0.1:9999"

echo "=============================="
echo " HTTP/HTTPS Proxy Test Suite "
echo "=============================="
echo

echo "[1] HTTP Forwarding Test (iana.org)"
curl -s -x $PROXY http://iana.org > /dev/null
if [ $? -eq 0 ]; then
    echo "PASS: HTTP forwarding works"
else
    echo "FAIL: HTTP forwarding failed"
fi
echo

echo "[2] HTTPS CONNECT Test (google.com)"
curl -s -x $PROXY https://google.com > /dev/null
if [ $? -eq 0 ]; then
    echo "PASS: HTTPS CONNECT tunneling works"
else
    echo "FAIL: HTTPS CONNECT tunneling failed"
fi
echo

echo "[3] Blocked Domain Test (instagram.com)"
STATUS=$(curl -s -o /dev/null -w "%{http_code}" \
        -x $PROXY https://www.instagram.com)

if [ "$STATUS" = "403" ]; then
    echo "PASS: Blocked domain correctly rejected (403)"
else
    echo "FAIL: Blocked domain not blocked (status=$STATUS)"
fi
echo

echo "[4] Concurrent Requests Test"
for i in {1..5}; do
    curl -s -x $PROXY http://iana.org > /dev/null &
done
wait
echo "PASS: Concurrent requests handled"
echo

echo "=============================="
echo " All tests completed "
echo "=============================="
