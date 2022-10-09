// SPDX-License-Identifier: MIT
pragma solidity >=0.4.22 <0.9.0;

contract Token {
    uint public ch;
    uint public t;
    constructor() {
        uint chainId;
        assembly {
            chainId := chainid()
        }
        ch = chainId;
        t = 0;
    }

    function getCh() external view returns(uint256) {
        // require(3 > 4, '3 is less than 4');
        return ch;
    }

    function getCh1() external returns(uint256) {
        require(4 > 5, '5 should be large than 4');
        t = 1;
        return ch;
    }

    function getCh2() external view returns(uint256) {
        require(false, 'qweqwe');
        return ch;
    }
}