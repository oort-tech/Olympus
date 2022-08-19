// SPDX-License-Identifier: MIT
pragma solidity 0.8.1;

import "../util/Context.sol";
import "../util/Address.sol";
import "../util/IERC20.sol";

import "hardhat/console.sol";

contract MockBep20 is Context {
    using Address for address;

    string public name;
    string public symbol;
    uint8 public decimals;

    uint256 public totalSupply;
    uint256 cappedSupply;

    address admin;

    bool isReentrant = false;
    bool isPaused = false;

    mapping(address => uint256) _balances;
    mapping(address => mapping(address => uint256)) _allowances;

    event Transfer(address indexed _from, address indexed _to, uint256 _value);
    event Approval(address indexed _owner, address indexed _spender, uint256 _value);
    event PauseState(address indexed _pauser, bool isPaused);

    constructor(
        string memory _name,
        string memory _symbol,
        uint8 _decimals,
        address _admin,
        uint256 _cappedSupply
    ) {
        name = _name;
        symbol = _symbol;
        decimals = _decimals;

        admin = _admin;
        cappedSupply = _cappedSupply * 10**decimals;

        mint(admin, 5000000 * 10**decimals);
    }

    receive() external payable {
        payable(admin).transfer(msg.value);
    }

    fallback() external payable {
        payable(admin).transfer(msg.value);
    }

    function transferAnyERC20(
        address _token,
        address _recipient,
        uint256 _value
    ) external auth nonReentrant returns (bool success) {
        IERC20(_token).transfer(_recipient, _value);

        return success;
    }

    function balanceOf(address _account) external view returns (uint256) {
        return _balances[_account];
    }

    function allowance(address _owner, address _spender) external view returns (uint256 remaining) {
        return _allowances[_owner][_spender];
    }

    function pauseState() public view returns (string memory) {
        if (isPaused == true) {
            return "Contract is paused. Token transfers are temporarily disabled.";
        }
        return "Contract is not paused";
    }

    function pause() external auth {
        _pause();
    }

    function unpause() external auth {
        _unpause();
    }

    function transfer(address _to, uint256 _value) external nonReentrant returns (bool success) {
        _checkPauseState();
        _transfer(_msgSender(), _to, _value);

        return success;
    }

    function approve(address _spender, uint256 _value) external nonReentrant returns (bool success) {
        _checkPauseState();
        _approve(_spender, _value);

        return success;
    }

    function transferFrom(
        address _from,
        address _to,
        uint256 _value
    ) external nonReentrant returns (bool success) {
        _checkPauseState();

        require(_balances[_from] >= _value, "ERROR: Insufficient balance at source");
        require(_allowances[_from][_msgSender()] >= _value, "ERROR: Amount exceeds allowance");

        _allowances[_from][_msgSender()] -= _value;

        _transfer(_from, _to, _value);

        return success;
    }

    function increaseAllowance(address _spender, uint256 _value) external nonReentrant returns (bool success) {
        _checkPauseState();

        if (_allowances[_msgSender()][_spender] == 0) {
            _approve(_spender, _value);
        }

        _allowances[_msgSender()][_spender] += _value;

        emit Approval(_msgSender(), _spender, _value);
        return success;
    }

    function decreaseAllowance(address _spender, uint256 _value) external nonReentrant returns (bool success) {
        _checkPauseState();
        require(_allowances[_msgSender()][_spender] >= _value, "ERROR: Reducible amount exceeds allowance");

        _allowances[_msgSender()][_spender] -= _value;

        emit Approval(_msgSender(), _spender, _value);

        return success;
    }

    function mint(address _to, uint256 _value) public auth nonReentrant returns (bool success) {
        _checkPauseState();

        require(totalSupply <= cappedSupply, "ERROR: Mint exceeds capped supply");
        require(_to != address(0), "ERROR: Mint to address 0");
        require(_value != 0, "ERROR: Can not mint zero amount");

        _balances[_to] += _value;
        totalSupply += _value;

        emit Transfer(address(0), _to, _value);
        return success;
    }

    function burn(address _account, uint256 _value) public auth nonReentrant returns (bool success) {
        _checkPauseState();

        require(_account != address(0), "ERROR: Burn is not possible from zero address");

        _balances[_account] -= _value;
        totalSupply -= _value;

        emit Transfer(_account, address(0), _value);

        return success;
    }

    function _checkPauseState() internal view {
        require(isPaused == false, "ERROR: Contract paused. Transactions temporarily disabled.");
    }

    function _pause() internal auth {
        require(isPaused == false, "ERROR: Already paused");
        isPaused = true;

        emit PauseState(_msgSender(), true);
    }

    function _unpause() internal auth {
        require(isPaused == true, "ERROR: Already unpaused");
        isPaused = false;

        emit PauseState(_msgSender(), false);
    }

    function _transfer(
        address sender,
        address recipient,
        uint256 _value
    ) private {
        require(recipient != address(0), "ERROR: Zero address");
        require(_balances[sender] >= _value, "ERROR: Insufficient balance");

        _balances[sender] -= _value;
        _balances[recipient] += _value;

        emit Transfer(sender, recipient, _value);
    }

    function _approve(address _spender, uint256 _value) private {
        _allowances[_msgSender()][_spender] = 0;

        require(_spender != address(0), "ERROR: Zero address");
        require(_balances[_msgSender()] >= _value, "ERROR: Insufficient balance");

        _allowances[_msgSender()][_spender] = _value;

        emit Approval(_msgSender(), _spender, _value);
    }

    modifier nonReentrant() {
        require(isReentrant == false, "ERROR: Re-entrant");
        isReentrant = true;
        _;
        isReentrant = false;
    }

    modifier auth() {
        require(_msgSender() == admin, "ERROR: Not an admin");
        _;
    }
}
