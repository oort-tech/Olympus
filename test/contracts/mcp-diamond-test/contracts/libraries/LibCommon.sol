// SPDX-License-Identifier: MIT
pragma solidity 0.8.1;

import "./AppStorageOpen.sol";

library LibCommon {
    address internal constant PANCAKESWAP_ROUTER_ADDRESS = 0x7437491D9348b7a12996464cE9cC716bA5A13f4A; // pancakeswap router address

    function upgradeAdmin() internal view returns (address upgradeAdmin_) {
        upgradeAdmin_ = diamondStorage().upgradeAdmin;
    }

    function diamondStorage() internal pure returns (AppStorageOpen storage ds) {
        assembly {
            ds.slot := 0
        }
    }

    function _isMarketSupported(bytes32 _market) internal view {
        AppStorageOpen storage ds = diamondStorage();
        require(ds.tokenSupportCheck[_market] == true, "ERROR: Unsupported market");
    }

    function _getMarketAddress(bytes32 _loanMarket) internal view returns (address) {
        AppStorageOpen storage ds = diamondStorage();
        return ds.indMarketData[_loanMarket].tokenAddress;
    }

    function _getMarketDecimal(bytes32 _loanMarket) internal view returns (uint256) {
        AppStorageOpen storage ds = diamondStorage();
        return ds.indMarketData[_loanMarket].decimals;
    }

   function _getTimelockValidityDeposit(/*bytes32 market, bytes32 commitment*/) internal view returns (uint256) {
        return diamondStorage().timelockValidity;
    }

    function _minAmountCheck(bytes32 _loanMarket, uint256 _amount) internal view {
        AppStorageOpen storage ds = diamondStorage();
        MarketData memory marketData = ds.indMarketData[_loanMarket];

        require(marketData.minAmount <= _amount, "ERROR: Less than minimum amount");
    }

    function _isMarket2Supported(bytes32 _loanMarket) internal view {
        require(diamondStorage().token2SupportCheck[_loanMarket] == true, "Secondary Token is not supported");
    }

    function _getMarket2Address(bytes32 _loanMarket) internal view returns (address) {
        AppStorageOpen storage ds = diamondStorage();
        return ds.indMarket2Data[_loanMarket].tokenAddress;
    }

    function _getMarket2Decimal(bytes32 _loanMarket) internal view returns (uint256) {
        AppStorageOpen storage ds = diamondStorage();
        return ds.indMarket2Data[_loanMarket].decimals;
    }

    function _connectMarket(bytes32 _market) internal view returns (address) {
        AppStorageOpen storage ds = diamondStorage();
        MarketData memory marketData = ds.indMarketData[_market];
        return marketData.tokenAddress;
    }

    function _getCommitment(uint256 _index, uint256 depositorborrow) internal view returns (bytes32) {
        AppStorageOpen storage ds = diamondStorage();
        if (depositorborrow == 1 && ds.borrowCommitment.length > _index) return ds.borrowCommitment[_index];
        else if (depositorborrow == 0 && ds.depositCommitment.length > _index) return ds.depositCommitment[_index];
        else revert("ERROR:Index Out of Bounds");
    }

    function _getApyTimeLength(bytes32 market, bytes32 _commitment) internal view returns (uint256) {
        AppStorageOpen storage ds = diamondStorage();
        return ds.indAPYRecords[market][_commitment].time.length;
    }

    function _getAprTimeLength(bytes32 market, bytes32 _commitment) internal view returns (uint256) {
        AppStorageOpen storage ds = diamondStorage();
        return ds.indAPRRecords[market][_commitment].time.length;
    }
}
