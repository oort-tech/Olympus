// SPDX-License-Identifier: MIT
pragma solidity 0.8.1;

interface ILoan1 {
    // enum STATE {ACTIVE,REPAID}
    function hasLoanAccount(address _account) external view returns (bool);

    function avblReservesLoan(bytes32 _market) external view returns (uint256);

    function utilisedReservesLoan(bytes32 _market) external view returns (uint256);

    function loanRequest(
        bytes32 _market,
        bytes32 _commitment,
        uint256 _loanAmount,
        bytes32 _collateralMarket,
        uint256 _collateralAmount
    ) external returns (bool);

    function pauseLoan1() external;

    function unpauseLoan1() external;

    function isPausedLoan1() external view returns (bool);
}
