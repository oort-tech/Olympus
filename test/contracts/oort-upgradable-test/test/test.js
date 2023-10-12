const { expect } = require("chai")
const { ethers, waffle } = require("hardhat")

describe('Fund Contract', function () {

    let owner, user, fundContract

    before(async () => {
        const [_owner, _user] = await ethers.getSigners()
        owner = _owner
        user = _user

        const FundContract = await ethers.getContractFactory('FundContract')
        fundContract = await FundContract.deploy()
        await fundContract.deployed()
    })

    it('deposit', async () => {
        const tx = await fundContract.connect(user).deposit({value: ethers.utils.parseEther('1')})
        await tx.wait()
        expect(await fundContract.getBalance(user.address)).to.equal(ethers.utils.parseEther('1'))
    })

    it('withdraw', async () => {
        const tx = await fundContract.connect(user).withdraw(ethers.utils.parseEther('0.5'))
        await tx.wait()
        expect(await fundContract.getBalance(user.address)).to.equal(ethers.utils.parseEther('0.5'))
        expect(await waffle.provider.getBalance(fundContract.address)).to.equal(ethers.utils.parseEther('0.5005'))
    })

    it('setFundFee', async () => {
        const tx = await fundContract.setFundFee(20)
        await tx.wait()
        expect(await fundContract.fundFee()).to.equal(20)
    })

    it('withdraw', async () => {
        const tx = await fundContract.connect(user).withdraw(ethers.utils.parseEther('0.5'))
        await tx.wait()
        expect(await fundContract.getBalance(user.address)).to.equal(0)
        expect(await waffle.provider.getBalance(fundContract.address)).to.equal(ethers.utils.parseEther('0.0015'))
    })
})