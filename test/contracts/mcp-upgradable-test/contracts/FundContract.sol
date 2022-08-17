// SPDX-License-Identifier: MIT
pragma solidity ^0.8.0;

import "@openzeppelin/contracts/access/Ownable.sol";

contract FundContract is Ownable {

    uint256 public fundFee;
    mapping(address => uint256) balances;

    constructor() {
        fundFee = 10; // means 0.1%
    }

    function getBalance(address owner) public view returns (uint256) {
        return balances[owner];
    }

    function deposit() public payable returns (uint256) {
        require(msg.value > 0, "No deposit");
        balances[msg.sender] += msg.value;
        return balances[msg.sender];
    }

    function calculateFee(uint256 amount) internal view returns (uint256 toUser, uint256 toFund) {
        toFund = amount * fundFee / 10000;
        toUser = amount - toFund;
    }

    function withdraw(uint256 amount) public returns (uint256) {
        require(balances[msg.sender] >= amount, "Insufficient balance");
        (uint256 toUser,) = calculateFee(amount);
        balances[msg.sender] -= amount;
        payable(msg.sender).transfer(toUser);
        return balances[msg.sender];
    }

    function setFundFee(uint256 percentage) public onlyOwner returns (uint256) {
        require(percentage < 10000, "Invalid value");
        fundFee = percentage;
        return fundFee;
    }
}