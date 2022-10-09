// SPDX-License-Identifier: MIT

pragma solidity 0.8.1;

import "./Context.sol";

abstract contract Pausable is Context {
    bool isReentrant = false;
    bool private isPaused;
    event PauseState(address indexed _pauser, bool isPaused);

    constructor() {
        isPaused = false;
    }

    modifier whenPaused() {
        require(_paused(), "Not paused status");
        _;
    }

    function _pause() internal virtual whenNotPaused {
        isPaused = true;
        emit PauseState(_msgSender(), true);
    }

    function _unpause() internal virtual whenPaused {
        isPaused = false;
        emit PauseState(_msgSender(), false);
    }

    function _paused() internal view virtual returns (bool) {
        return isPaused;
    }

    function _checkPauseState() internal view {
        require(isPaused == false, "The contract is paused. Transfer functions are temporarily disabled");
    }

    modifier nonReentrant() {
        require(isReentrant == false, "ERROR: Re-entrant");
        isReentrant = true;
        _;
        isReentrant = false;
    }

    modifier whenNotPaused() {
        require(!_paused(), "Paused status");
        _;
    }
}
