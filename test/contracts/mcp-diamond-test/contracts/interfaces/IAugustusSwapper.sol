// SPDX-License-Identifier: MIT
pragma solidity 0.8.1;

interface IAugustusSwapper {
    function simpleSwap(
        address fromToken,
        address toToken,
        uint256 fromAmount,
        uint256 toAmount,
        uint256 expectedAmount,
        address[] memory callees,
        bytes memory exchangeData,
        uint256[] memory startIndexes,
        uint256[] memory values,
        address payable beneficiary,
        string memory referrer,
        bool useReduxToken
    ) external payable returns (uint256 receivedAmount);

    function swapOnUniswap(
        uint256 amountIn,
        uint256 amountOutMin,
        address[] calldata path,
        uint8 referrer
    ) external payable returns (uint256 receivedAmount);
}
