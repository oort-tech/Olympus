// SPDX-License-Identifier: MIT
pragma solidity ^0.8.0;

import '@openzeppelin/contracts/access/Ownable.sol';
import '@openzeppelin/contracts/token/ERC20/ERC20.sol';

contract UCCN is Ownable, ERC20 {

    constructor() ERC20("UCCN", "UCCN") {
        _mint(_msgSender(), 10000000 * 10 ** decimals());
    }
    
    function decimals() public pure override returns (uint8) {
        return 6;
    }
    
    function mint(uint256 amount) public onlyOwner {
        _mint(_msgSender(), amount);
    }
}