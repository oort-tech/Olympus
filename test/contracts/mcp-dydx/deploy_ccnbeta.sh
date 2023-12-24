#!/bin/bash

# Get the current Node.js version
node_version=$(node -v)

# Check if the Node.js version starts with "v20."
if [[ $node_version == v20.* ]]; then
  echo "Node.js version is 20. Proceeding with npm run deploy_ccnbeta..."
  npm run deploy_ccnbeta
else
  echo "Node.js version is not 20. Please switch to Node.js version 20 to run npm run deploy_ccnbeta."
fi
