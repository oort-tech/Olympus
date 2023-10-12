// SPDX-License-Identifier: MIT
pragma solidity ^0.8.0;

import "@openzeppelin/contracts/utils/math/SafeMath.sol";
import "@openzeppelin/contracts/token/ERC20/IERC20.sol";
import "@openzeppelin/contracts/access/Ownable.sol";

interface USDToken is IERC20 {
    function decimals() external returns (uint256);
}

contract EarlyBirdCollateral is Ownable {
    using SafeMath for uint256;

    USDToken USDT;
    mapping(address => bool) authorizedAccount; // Here the address is Ethereum address

    mapping(address => uint256) public quota; // Here the address is Ethereum address
    mapping(address => uint256) public balances; // Here the address is Ethereum address
    mapping(address => bool) public userRefunded; // Here the address is Ethereum address

    mapping(address => address) public hgn_eth_map; // Map from the Huygens address to Ethereum address
    mapping(address => address) public eth_hgn_map; // Map from the Ethereum address to Huygens address

    event SetQuota(
        address indexed eth_addr,
        address indexed hgn_addr,
        uint256 indexed amount
    );
    event Deposit(
        address indexed eth_addr,
        address indexed hgn_addr,
        uint256 indexed amount
    );

    modifier onlyAuthorized() {
        require(authorizedAccount[msg.sender], "Not authroized account");
        _;
    }

    constructor(address _USDT) {
        USDT = USDToken(_USDT);
    }

    function setAuthorized(address account, bool result) public onlyOwner {
        authorizedAccount[account] = result;
    }

    function totalBalance() public view returns (uint256) {
        return USDT.balanceOf(address(this));
    }

    function setQuota(
        address eth_addr,
        address hgn_addr,
        uint256 amount
    ) public onlyAuthorized {
        uint256 decimals = 10**USDT.decimals();
        require(amount > 0, "Quota amount should be greater than 0.");
        require(
            userRefunded[eth_addr] == false && quota[eth_addr] == 0 && balances[eth_addr] == 0,
            "The Ethereum address is taken."
        );
        require(
            quota[hgn_eth_map[hgn_addr]] == 0 &&
                balances[hgn_eth_map[hgn_addr]] == 0,
            "The Huygens address is taken."
        );
        require(
            amount >= uint256(150000).mul(decimals),
            "The minimum deposit amount is 150000"
        );
        quota[eth_addr] = amount;
        hgn_eth_map[hgn_addr] = eth_addr;
        eth_hgn_map[eth_addr] = hgn_addr;

        emit SetQuota(eth_addr, hgn_addr, amount);
    }

    function deposit(uint256 amount) public {
        uint256 decimals = 10**USDT.decimals();
        require(amount > 0, "Deposit amount should be greater than 0.");
        if (totalBalance().add(amount) < uint256(15000000).mul(decimals))
            require(
                amount == quota[msg.sender],
                "Deposit amount doesn't match quota."
            );
        else
            require(
                totalBalance().add(amount) == uint256(15000000).mul(decimals),
                "Total deposit balance cannot exceed 15 million."
            );
        balances[msg.sender] = balances[msg.sender].add(amount);
        quota[msg.sender] = 0;
        USDT.transferFrom(msg.sender, address(this), amount);

        emit Deposit(msg.sender, eth_hgn_map[msg.sender], amount);
    }

    function refund(address receiver, uint256 price) public onlyOwner {
        uint256 decimals = 10**USDT.decimals();
        require(userRefunded[receiver] == false, "Already refunded");
        require(price <= uint256(3).mul(decimals), "Incorrect price");
        uint256 amount = balances[receiver].mul(decimals.sub(price.div(3))).div(
            decimals
        );
        balances[receiver] = balances[receiver].sub(amount);
        userRefunded[receiver] = true;
        USDT.transfer(receiver, amount);
    }

    function withraw(uint256 amount) public onlyOwner {
        USDT.transfer(owner(), amount);
    }

    function withrawAll() public onlyOwner {
        USDT.transfer(owner(), USDT.balanceOf(address(this)));
    }
}
