// SPDX-License-Identifier: MIT
pragma solidity 0.8.1;

// import "./IAugustusSwapper.sol";
// import "./ITokenList.sol";

interface ILiquidator {
    function liquidation(
        address account,
        bytes32 _market,
        bytes32 _commitment
    ) external returns (bool success);

    function liquidableLoans(uint256 _indexFrom)
        external
        view
        returns (
            address[] memory loanOwner,
            bytes32[] memory loanMarket,
            bytes32[] memory loanCommitment,
            uint256[] memory loanAmount,
            bytes32[] memory collateralMarket,
            uint256[] memory collateralAmount
        );

    function pauseLiquidator() external;

    function unpauseLiquidator() external;

    function isPausedLiquidator() external view returns (bool);
}
