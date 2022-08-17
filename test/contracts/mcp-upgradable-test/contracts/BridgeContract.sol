// SPDX-License-Identifier: MIT
pragma solidity ^0.8.0;

import "@openzeppelin/contracts/utils/math/SafeMath.sol";
import "@openzeppelin/contracts/token/ERC20/IERC20.sol";
import "@openzeppelin/contracts-upgradeable/access/OwnableUpgradeable.sol";

contract BridgeContract is OwnableUpgradeable {
    using SafeMath for uint256;

    address operationAccount;
    address public wrapAddress; // wrap token address for coin

    mapping(address => bool) authorizedAccount;
    mapping(address => uint256) public balanceAdjustmentQuota;
    mapping(address => uint256) public crossChainFee;
    mapping(address => uint256) public balanceCrossChainFee;

    function initialize(address _operationAccount, address _wrapAddress, address _authorizedAccount) public initializer {
        __Ownable_init();
        operationAccount = _operationAccount;
        wrapAddress = _wrapAddress;
        authorizedAccount[_authorizedAccount] = true;
    }

    modifier authorizedOnly() {
        require(authorizedAccount[msg.sender], "Not authroized account");
        _;
    }

    modifier operationOnly() {
        require(msg.sender == operationAccount, "Not operation account");
        _;
    }

    function setCrossChainFee(address erc20, uint256 fee) public operationOnly {
        crossChainFee[erc20] = fee;
    }

    event Deposit(address indexed erc20, uint256 indexed amount, string target);
    event ResetQuta(address indexed erc20, uint256 indexed remain);
    event Inject(address indexed erc20, uint256 indexed amount);

    function deposit(address erc20, uint256 amount, string memory target) public {
        require(amount > crossChainFee[erc20], "No deposit amount");
        balanceCrossChainFee[erc20] = balanceCrossChainFee[erc20].add(crossChainFee[erc20]);
        IERC20(erc20).transferFrom(msg.sender, address(this), amount);
        emit Deposit(erc20, amount, target);
    }

    function depositWithCoin(string memory target) public payable {
        require(msg.value > crossChainFee[address(0)], "No deposit amount");
        balanceCrossChainFee[address(0)] = balanceCrossChainFee[address(0)].add(crossChainFee[address(0)]);
        emit Deposit(address(0), msg.value, target);
    }

    /**
     * @dev Called by owner to withraw cross chain fee
     */
    function withraw(address erc20, uint256 amount) public onlyOwner {
        require(balanceCrossChainFee[erc20] >= amount, "Insufficient available balance");
        balanceCrossChainFee[erc20] = balanceCrossChainFee[erc20].sub(amount);
        if (erc20 == address(0)) {
            (bool success, ) = msg.sender.call{value: amount}("");
            require(success, "Failed to transfer value");
        } else {
            IERC20(erc20).transfer(msg.sender, amount);
        }
    }

    /**
     * @dev Called from nodejs wallet when captured event from depositor
     * if erc20 is 0, this is for wrap -> coin
     * else if erc20 is wrapAddress, this is for coin -> wrap
     * else this is for erc20 -> erc20
     */
    function addAvailableBalanceWithAdjustmentQuota(address erc20, uint256 amount, address target) public operationOnly {
        require(amount > crossChainFee[erc20], "No deposit amount");
        require(balanceAdjustmentQuota[erc20] > amount, "Insufficient available quata");
        // availableBalances[target][erc20] = availableBalances[target][erc20].add(amount);
        // totalAvailableBalances[erc20] = totalAvailableBalances[erc20].add(amount);
        balanceAdjustmentQuota[erc20] = balanceAdjustmentQuota[erc20].sub(amount);
        if (balanceAdjustmentQuota[erc20] < 1 ether) { // this is sample threshold, it must be picked by exact value
            emit ResetQuta(erc20, balanceAdjustmentQuota[erc20]);
        }

        uint256 realBalance = 0;
        if (erc20 == address(0)) {
            (bool success, ) = target.call{value: amount.sub(crossChainFee[erc20])}("");
            require(success, "Failed to transfer value");
            realBalance = address(this).balance;
        } else {
            IERC20(erc20).transfer(target, amount.sub(crossChainFee[erc20]));
            realBalance = IERC20(erc20).balanceOf(address(this));
        }
        if (realBalance < 1 ether) { // this is sample threshold, it must be picked by exact value
            emit Inject(erc20, realBalance);
        }
    }

    /**
     * @dev Called when you want to reset quota of token for nodejs wallet
     * if erc20 is 0, it means coin
     */
    function resetBalanceAdjustmentQuota(address erc20, uint256 amount) public authorizedOnly {
        balanceAdjustmentQuota[erc20] = amount;
    }

    /**
     * @dev Called when you want to inject certain amount of token for bridging
     * if erc20 is 0, it means coin
     */
    function inject(address erc20, uint256 amount) public payable authorizedOnly {
        if (erc20 == address(0)) {
            require(msg.value == amount, "Invalid parameter");
        } else {
            IERC20(erc20).transferFrom(msg.sender, address(this), amount);
        }
    }
}