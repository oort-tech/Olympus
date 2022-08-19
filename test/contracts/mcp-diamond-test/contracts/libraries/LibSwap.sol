// SPDX-License-Identifier: MIT
pragma solidity 0.8.1;

import "./LibCommon.sol";
import { IPancakeRouter01 } from "../interfaces/IPancakeRouter01.sol";
import "hardhat/console.sol";

library LibSwap {
    function _getMarketAddress(bytes32 _loanMarket) internal view returns (address) {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        return ds.indMarketData[_loanMarket].tokenAddress;
    }

    function _getMarket2Address(bytes32 _loanMarket) internal view returns (address) {
        AppStorageOpen storage ds = LibCommon.diamondStorage();
        return ds.indMarket2Data[_loanMarket].tokenAddress;
    }

    function _swap(
        bytes32 _fromMarket,
        bytes32 _toMarket,
        uint256 _fromAmount,
        uint8 _mode
    ) internal returns (uint256) {
        if (_fromMarket == _toMarket) return _fromAmount;
        address addrFromMarket;
        address addrToMarket;
        address addrWbnb;

        addrWbnb = _getMarketAddress(0x57424e4200000000000000000000000000000000000000000000000000000000);
        require(addrWbnb != address(0), "WBNB Address can not be zero.");

        if (_mode == 0) {
            addrFromMarket = _getMarketAddress(_fromMarket);
            addrToMarket = _getMarket2Address(_toMarket);
        } else if (_mode == 1) {
            addrFromMarket = _getMarket2Address(_fromMarket);
            addrToMarket = _getMarketAddress(_toMarket);
        } else if (_mode == 2) {
            addrFromMarket = _getMarketAddress(_fromMarket);
            addrToMarket = _getMarketAddress(_toMarket);
        }

        require(addrFromMarket != address(0) && addrToMarket != address(0), "Swap Address can not be zero.");

        //PancakeSwap
        IBEP20(addrFromMarket).approve(LibCommon.PANCAKESWAP_ROUTER_ADDRESS, _fromAmount);

        address[] memory path;

        if (addrFromMarket == addrWbnb || addrToMarket == addrWbnb) {
            path = new address[](2);
            path[0] = addrFromMarket;
            path[1] = addrToMarket;
        } else {
            path = new address[](3);
            path[0] = addrFromMarket;
            path[1] = addrWbnb;
            path[2] = addrToMarket;
        }

        // https://github.com/pancakeswap/pancake-document/blob/c3531149a4b752a0cfdf94f2d276ac119f89774b/code/smart-contracts/pancakeswap-exchange/router-v2.md#swapexacttokensfortokens
        uint256[] memory ret;
        ret = IPancakeRouter01(LibCommon.PANCAKESWAP_ROUTER_ADDRESS).swapExactTokensForTokens(
            _fromAmount,
            _getAmountOutMin(addrFromMarket, addrToMarket, _fromAmount),
            path,
            address(this),
            block.timestamp + 15
        );
        return ret[ret.length - 1];
    }

    function _getAmountOutMin(
        address _tokenIn,
        address _tokenOut,
        uint256 _amountIn
    ) internal view returns (uint256) {
        if (_amountIn == 0) {
            return 0;
        }
        if (_tokenIn == _tokenOut) {
            return _amountIn;
        }
        address addrWbnb;
        addrWbnb = _getMarketAddress(0x57424e4200000000000000000000000000000000000000000000000000000000);
        require(addrWbnb != address(0), "WBNB Address can not be zero.");

        address[] memory path;

        if (_tokenIn == addrWbnb || _tokenOut == addrWbnb) {
            path = new address[](2);
            path[0] = _tokenIn;
            path[1] = _tokenOut;
        } else {
            path = new address[](3);
            path[0] = _tokenIn;
            path[1] = addrWbnb;
            path[2] = _tokenOut;
        }

        // same length as path
        uint256[] memory amountOutMins = IPancakeRouter01(LibCommon.PANCAKESWAP_ROUTER_ADDRESS).getAmountsOut(
            _amountIn,
            path
        );
        return amountOutMins[path.length - 1];
    }
}
