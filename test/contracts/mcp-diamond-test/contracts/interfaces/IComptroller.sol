// SPDX-License-Identifier: MIT
pragma solidity 0.8.1;

interface IComptroller {
    function getAPR(bytes32 market, bytes32 commitment_) external view returns (uint256);

    function getAPRInd(
        bytes32 market,
        bytes32 _commitment,
        uint256 index
    ) external view returns (uint256);

    function getAPY(bytes32 market, bytes32 _commitment) external view returns (uint256);

    function getAPYInd(
        bytes32 market,
        bytes32 _commitment,
        uint256 _index
    ) external view returns (uint256);

    function getApytime(
        bytes32 market,
        bytes32 _commitment,
        uint256 _index
    ) external view returns (uint256);

    function getAprtime(
        bytes32 market,
        bytes32 _commitment,
        uint256 _index
    ) external view returns (uint256);

    function getApyLastTime(bytes32 market, bytes32 commitment_) external view returns (uint256);

    function getAprLastTime(bytes32 market, bytes32 commitment_) external view returns (uint256);

    function getApyTimeLength(bytes32 market, bytes32 commitment_) external view returns (uint256);

    function getAprTimeLength(bytes32 market, bytes32 commitment_) external view returns (uint256);

    function getCommitment(uint256 index_, uint256 depositorborrow) external view returns (bytes32);

    function getTimelockValidityDeposit() external view returns (uint256);

    function setDepositCommitment(bytes32 _commitment, uint256 _days) external;

    function setBorrowCommitment(bytes32 _commitment, uint256 _days) external;

    function setTimelockValidityDeposit(uint256 time) external returns (bool success);

    function updateLoanIssuanceFees(uint256 fees) external returns (bool success);

    function updateLoanClosureFees(uint256 fees) external returns (bool success);

    function updateCollateralPreclosureFees(uint256 fees) external returns (bool success);

    function updateLoanPreClosureFees(uint256 fees) external returns (bool success);

    function updateDepositPreclosureFees(uint256 fees) external returns (bool success);

    function updateWithdrawalFees(uint256 fees) external returns (bool success);

    function updateCollateralReleaseFees(uint256 fees) external returns (bool success);

    function updateYieldConversion(uint256 fees) external returns (bool success);

    function updateMarketSwapFees(uint256 fees) external returns (bool success);

    function updateReserveFactor(uint256 _reserveFactor) external returns (bool success);

    function updateMaxWithdrawal(uint256 factor, uint256 blockLimit) external returns (bool success);

    function getReserveFactor() external view returns (uint256);

    function pauseComptroller() external;

    function unpauseComptroller() external;

    function isPausedComptroller() external view returns (bool);

    function collateralPreClosureFees() external view returns (uint256);

    function depositPreClosureFees() external view returns (uint256);

    function depositWithdrawalFees() external view returns (uint256);

    function collateralReleaseFees() external view returns (uint256);
}
