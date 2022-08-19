// SPDX-License-Identifier: MIT
pragma solidity 0.8.1;

interface ILoan2 {
    // enum STATE {ACTIVE,REPAID}
    function repayLoan(
        bytes32 _market,
        bytes32 _commitment,
        uint256 _repayAmount
    ) external returns (bool);

    function pauseLoan2() external;

    function unpauseLoan2() external;

    function isPausedLoan2() external view returns (bool);
}
