pragma solidity ^0.5.16;

import "./MIEther.sol";

/**
 * @title MarketIndicator's Maximillion Contract
 * @author MarketIndicator
 */
contract Maximillion {
    /**
     * @notice The default miEther market to repay in
     */
    MIEther public miEther;

    /**
     * @notice Construct a Maximillion to repay max in a MIEther market
     */
    constructor(MIEther miEther_) public {
        miEther = miEther_;
    }

    /**
     * @notice msg.sender sends Ether to repay an account's borrow in the miEther market
     * @dev The provided Ether is applied towards the borrow balance, any excess is refunded
     * @param borrower The address of the borrower account to repay on behalf of
     */
    function repayBehalf(address borrower) public payable {
        repayBehalfExplicit(borrower, miEther);
    }

    /**
     * @notice msg.sender sends Ether to repay an account's borrow in a miEther market
     * @dev The provided Ether is applied towards the borrow balance, any excess is refunded
     * @param borrower The address of the borrower account to repay on behalf of
     * @param miEther_ The address of the miEther contract to repay in
     */
    function repayBehalfExplicit(address borrower, MIEther miEther_) public payable {
        uint received = msg.value;
        uint borrows = miEther_.borrowBalanceCurrent(borrower);
        if (received > borrows) {
            miEther_.repayBorrowBehalf.value(borrows)(borrower);
            msg.sender.transfer(received - borrows);
        } else {
            miEther_.repayBorrowBehalf.value(received)(borrower);
        }
    }
}
