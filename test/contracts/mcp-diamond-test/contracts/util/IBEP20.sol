// SPDX-License-Identifier: MIT
pragma solidity 0.8.1;

interface IBEP20 {
    function transfer(address _to, uint256 _value) external returns (bool);

    function approve(address _spender, uint256 _value) external returns (bool);

    function approveFrom(
        address _sender,
        address _spender,
        uint256 _value
    ) external returns (bool);

    function allowance(address _owner, address _spender) external view returns (uint256 remaining);

    function increaseAllowance(address _spender, uint256 _value) external returns (bool);

    function decreaseAllowance(address _spender, uint256 _value) external returns (bool);

    function transferFrom(
        address _from,
        address _to,
        uint256 _value
    ) external returns (bool);

    function pauseState() external view returns (string memory);

    function balanceOf(address account) external view returns (uint256);
}
