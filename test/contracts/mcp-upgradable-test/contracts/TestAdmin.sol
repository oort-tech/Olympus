// SPDX-License-Identifier: MIT
pragma solidity 0.6.12;

import './TestImpl.sol';

contract TestAdmin {

    string public content;
    string public symbol;

    struct PoolInfo {
        TestImpl lpToken;           // Address of LP token contract.
        uint256 allocPoint;       // How many allocation points assigned to this pool. CAKEs to distribute per block.
        uint256 lastRewardBlock;  // Last block number that CAKEs distribution occurs.
        uint256 accCakePerShare; // Accumulated CAKEs per share, times 1e12. See below.
    }

    PoolInfo[] public poolInfo;

    TestImpl t;

    constructor(TestImpl _impl) public {
        poolInfo.push(PoolInfo({
            lpToken: _impl,
            allocPoint: 1000,
            lastRewardBlock: 0,
            accCakePerShare: 0
        }));

        // t = new TestImpl();
        t = _impl;
    }

    function getSymbol() public view returns (string memory) {
        return t.symbol();
    }

    function getContent() public view returns (string memory) {
        return t.content();
    }

    function setContent(address a, string memory _content) public {
        // t.setContent(_content);
        require(false, "error");
        bytes memory payload = abi.encodeWithSelector(TestImpl(a).setContent.selector, _content);
        (bool success, ) = a.delegatecall(payload);
        require(success);
    }

    function poolLength() public view returns (uint256) {
        return poolInfo.length;
    }

    function add(TestImpl _impl) public {
        poolInfo.push(PoolInfo({
            lpToken: _impl,
            allocPoint: 1000,
            lastRewardBlock: block.number, // you can set as "0" instead of block.number
            accCakePerShare: 0
        }));
    }

    function update(uint256 id) public {
        PoolInfo storage pool = poolInfo[id];

        // case 1
        pool.lastRewardBlock = block.number;

        // case 2
        // pool.lastRewardBlock = 0;

        // case 3
        // if (block.number == pool.lastRewardBlock) {
        //     return;
        // }
        // pool.lpToken.symbol();
    }
}