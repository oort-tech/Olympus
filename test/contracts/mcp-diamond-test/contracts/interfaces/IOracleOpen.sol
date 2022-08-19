// SPDX-License-Identifier: MIT
pragma solidity 0.8.1;

interface IOracleOpen {
    function getQuote(bytes32 _market) external returns (uint256);

    function getFairPrice(uint256 _requestId) external returns (uint256);

    function pauseOracle() external;

    function unpauseOracle() external;

    function isPausedOracle() external view returns (bool);
}
