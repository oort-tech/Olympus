const { expect } = require("chai")
const { ethers, upgrades, waffle } = require("hardhat")

describe('Bridge contract', function () {

    let owner, operation, authorized, receiver
    let erc20, ccnwrap, bridge

    before(async () => {
        const ERC20 = await ethers.getContractFactory('TetherToken')
        erc20 = await ERC20.deploy(1000000000000, 'tether', 'usdm', 4)
        await erc20.deployed()

        const CCNWRAP = await ethers.getContractFactory('UCCN')
        ccnwrap = await CCNWRAP.deploy()
        await ccnwrap.deployed()

        const [_owner, _operation, _authorized, _receiver] = await ethers.getSigners()
        owner = _owner
        operation = _operation
        authorized = _authorized
        receiver = _receiver

        const Bridge = await ethers.getContractFactory("BridgeContract")
        bridge = await upgrades.deployProxy(Bridge, [operation.address, ccnwrap.address, authorized.address])
        await bridge.deployed()
    })

    it('setCrossChainFee', async () => {
        await bridge.connect(operation).setCrossChainFee('0x0000000000000000000000000000000000000000', ethers.utils.parseEther("0.01"))
        await bridge.connect(operation).setCrossChainFee(ccnwrap.address, 10)
        await bridge.connect(operation).setCrossChainFee(erc20.address, 10)
    })

    it('deposit : erc20', async () => {
        await erc20.approve(bridge.address, 100)
        await bridge.deposit(erc20.address, 100, "0x90F79bf6EB2c4f870365E785982E1f101E93b906")
        expect(await erc20.balanceOf(bridge.address)).to.equal(100)
    })

    it('deposit : CCNWrap', async () => {
        await ccnwrap.approve(bridge.address, 100)
        await bridge.deposit(ccnwrap.address, 100, "0x90F79bf6EB2c4f870365E785982E1f101E93b906")
        expect(await ccnwrap.balanceOf(bridge.address)).to.equal(100)
    })

    it('deposit : Coin', async () => {
        await bridge.depositWithCoin("0x90F79bf6EB2c4f870365E785982E1f101E93b906", {value: ethers.utils.parseEther("1")})
        expect(await waffle.provider.getBalance(bridge.address)).to.equal(ethers.utils.parseEther("1"))
    })

    it('addAvailableBalanceWithAdjustmentQuota : erc20->erc20', async () => {
        await bridge.connect(authorized).resetBalanceAdjustmentQuota(erc20.address, 1000)
        expect(await bridge.balanceAdjustmentQuota(erc20.address)).to.equal(1000)

        await bridge.connect(operation).addAvailableBalanceWithAdjustmentQuota(erc20.address, 100, receiver.address)
        expect(await erc20.balanceOf(receiver.address)).to.equal(90)
    })

    it('addAvailableBalanceWithAdjustmentQuota : wrap->coin', async () => {
        await bridge.connect(authorized).resetBalanceAdjustmentQuota(ccnwrap.address, 1000)
        expect(await bridge.balanceAdjustmentQuota(ccnwrap.address)).to.equal(1000)

        await bridge.connect(operation).addAvailableBalanceWithAdjustmentQuota(ccnwrap.address, 100, receiver.address)
        expect(await ccnwrap.balanceOf(receiver.address)).to.equal(90)
    })

    it('addAvailableBalanceWithAdjustmentQuota : coin->wrap', async () => {
        await bridge.connect(authorized).resetBalanceAdjustmentQuota('0x0000000000000000000000000000000000000000', ethers.utils.parseEther("10"))
        expect(await bridge.balanceAdjustmentQuota('0x0000000000000000000000000000000000000000')).to.equal(ethers.utils.parseEther("10"))

        await bridge.connect(operation).addAvailableBalanceWithAdjustmentQuota('0x0000000000000000000000000000000000000000', ethers.utils.parseEther("1"), receiver.address)
        expect(await waffle.provider.getBalance(receiver.address)).to.equal(ethers.utils.parseEther("10000.99"))
    })

    it('inject : erc20', async () => {
        await erc20.connect(owner).transfer(authorized.address, 500)
        await erc20.connect(authorized).approve(bridge.address, 150)
        await bridge.connect(authorized).inject(erc20.address, 150)
        expect(await erc20.balanceOf(bridge.address)).to.equal(160)
    })

    it('inject : CCNWrap', async () => {
        await ccnwrap.connect(owner).transfer(authorized.address, 500)
        await ccnwrap.connect(authorized).approve(bridge.address, 150)
        await bridge.connect(authorized).inject(ccnwrap.address, 150)
        expect(await ccnwrap.balanceOf(bridge.address)).to.equal(160)
    })
    
    it('inject : Coin', async () => {
        await bridge.connect(authorized).inject('0x0000000000000000000000000000000000000000', ethers.utils.parseEther("1"), {value: ethers.utils.parseEther("1")})
        expect(await waffle.provider.getBalance(bridge.address)).to.equal(ethers.utils.parseEther("1.01"))
    })

    it('withraw', async () => {
        expect(await bridge.balanceCrossChainFee('0x0000000000000000000000000000000000000000')).to.equal(ethers.utils.parseEther("0.01"))
        await bridge.withraw('0x0000000000000000000000000000000000000000', await bridge.balanceCrossChainFee('0x0000000000000000000000000000000000000000'))
        expect(await bridge.balanceCrossChainFee('0x0000000000000000000000000000000000000000')).to.equal(0)

        expect(await bridge.balanceCrossChainFee(erc20.address)).to.equal(10)
        await bridge.withraw(erc20.address, await bridge.balanceCrossChainFee(erc20.address))
        expect(await bridge.balanceCrossChainFee(erc20.address)).to.equal(0)

        expect(await bridge.balanceCrossChainFee(ccnwrap.address)).to.equal(10)
        await bridge.withraw(ccnwrap.address, await bridge.balanceCrossChainFee(ccnwrap.address))
        expect(await bridge.balanceCrossChainFee(ccnwrap.address)).to.equal(0)
    })
})