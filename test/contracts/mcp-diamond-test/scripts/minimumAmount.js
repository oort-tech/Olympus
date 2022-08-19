const symbolWBNB = "0x57424e4200000000000000000000000000000000000000000000000000000000"; // WBNB
const symbolUsdt = "0x555344542e740000000000000000000000000000000000000000000000000000"; // USDT.t
const symbolUsdc = "0x555344432e740000000000000000000000000000000000000000000000000000"; // USDC.t
const symbolBtc = "0x4254432e74000000000000000000000000000000000000000000000000000000"; // BTC.

function minAmount(market, chainId) {
  if (chainId == 1337) {
    if (market == symbolWBNB) return 25000000;
    else if (market == symbolUsdt || market == symbolUsdc) return 250000000000;
    else return 10000000;
  } else {
    if (market == symbolWBNB) return 250000000;
    else if (market == symbolUsdt || market == symbolUsdc) return 250000000000;
    else return 100000000;
  }
}

exports.minAmount = minAmount;
