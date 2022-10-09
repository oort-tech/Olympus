// SPDX-License-Identifier: MIT
pragma solidity 0.8.1;

interface IDynamicInterest {
    function getDepositInterests(uint256 _minOrMax) external view returns (uint256);

    function getBorrowInterests(uint256 _minOrMax) external view returns (uint256);

    function getInterestFactors(uint256 _factor) external view returns (uint256);

    function setDepositInterests(uint256 minDepositInterest, uint256 maxDepositInterest) external returns (bool);

    function setBorrowInterests(uint256 minBorrowInterest, uint256 maxBorrowInterest) external returns (bool);

    function setInterestFactors(uint256 offset, uint256 correlationFactor) external returns (bool);

    function updateInterests(bytes32 market) external returns (bool);

    function pauseDynamicInterest() external;

    function unpauseDynamicInterest() external;

    function isPausedDynamicInterest() external view returns (bool);
}
