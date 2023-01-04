FROM dydxprotocol/node:8.12-alpine-v1

RUN mkdir -p /home/dydx/app
WORKDIR /home/dydx/app
COPY . .

RUN mkdir /home/.ganache
EXPOSE 8545
