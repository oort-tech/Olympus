// SPDX-License-Identifier: MIT
pragma solidity 0.8.1;

interface IReserve {
    function transferAnyBEP20(
        address token_,
        address recipient_,
        uint256 value_
    ) external returns (bool);

    function avblMarketReserves(bytes32 _market) external view returns (uint256);

    function marketReserves(bytes32 _market) external view returns (uint256);

    function marketUtilisation(bytes32 _market) external view returns (uint256);

    function avblReservesDeposit(bytes32 _market) external view returns (uint256);

    function utilisedReservesDeposit(bytes32 _market) external view returns (uint256);

    function pauseReserve() external;

    function unpauseReserve() external;

    function isPausedReserve() external view returns (bool);
}
