// SPDX-License-Identifier: MIT
pragma solidity 0.8.1;

interface BEP20 {
    function transfer(address to, uint256 value) external returns (bool);

    function balanceOf(address account) external view returns (uint256);

    event Transfer(address indexed from, address indexed to, uint256 value);
}

contract Faucet {
    uint256 public num = 0;
    address admin;

    bool isReentrant = false;
    uint256 public waitTime = 1440 minutes;

    struct TokenLedger {
        BEP20 token;
        uint256 amount;
        uint256 balance;
    }

    mapping(uint256 => TokenLedger) tokens; // token id => token ledger.
    mapping(address => mapping(BEP20 => uint256)) airdropRecords; // Address to token to amount of tokens to drip.

    event TokensIssued(BEP20 indexed token, address indexed account, uint256 indexed amount, uint256 time);
    event TokenSupportAdded(BEP20 indexed token, uint256 indexed amount, uint256 indexed value, uint256 time);

    constructor() {
        admin = msg.sender;
    }

    /// UPDATE TOKENS
    function _updateTokens(
        address _token,
        uint256 _amount,
        uint256 _value
    ) public auth returns (bool) {
        require(_token != address(0), "ERROR: Zero address");

        TokenLedger storage td = tokens[num];
        td.token = BEP20(_token); // token pointer
        td.amount = _value; // drip amount
        td.balance = _amount; // Faucet balance

        num += 1;

        emit TokenSupportAdded(td.token, td.amount, td.balance, block.timestamp);
        return true;
    }

    /// GET TOKENS
    function getTokens(uint256 _index) public payable nonReentrant returns (bool success) {
        require(msg.sender != address(0), "ERROR: Zero address");

        TokenLedger storage td = tokens[_index];
        td.balance = td.token.balanceOf(address(this)); // Faucet balance

        require(td.balance >= td.amount, "ERROR: Insufficient balance");
        require(airdropRecords[msg.sender][td.token] <= block.timestamp, "ERROR: Active timelock");

        td.token.transfer(msg.sender, td.amount);
        td.balance -= td.amount;

        airdropRecords[msg.sender][td.token] = block.timestamp + waitTime;

        emit TokensIssued(td.token, msg.sender, td.amount, block.timestamp);
        return success = true;
    }

    modifier nonReentrant() {
        require(isReentrant == false, "ERROR: Re-entrant");
        isReentrant = true;
        _;
        isReentrant = false;
    }

    modifier auth() {
        require(msg.sender == admin, "ERROR: Not authorized");
        _;
    }
}
