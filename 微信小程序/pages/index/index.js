/**
 * 仓库多认证门禁门锁小程序 - 与OneNET云平台/STM32门锁设备交互
 *
 * 物模型对齐(与单片机端一致):
 *   上报(只读): LockState(0开1关) AuthType(1人脸2NFC3密码4远程) AuthResult(true/false) FaceCount(数量)
 *   下发(读写): Face_Clear(清除人脸库) Password_Set(6位密码) RemoteUnlock(远程开门)
 *
 * 交互模式: OneNET HTTP API
 *   GET  /thingmodel/query-device-property  3秒轮询物模型属性
 *   POST /thingmodel/set-device-property    下发控制指令
 */
const util = require('../../utils/util.js')

const AUTH_TYPE_MAP = {
  1: '人脸识别',
  2: 'NFC刷卡',
  3: '密码验证',
  4: '远程开锁'
}

Page({
  data: {
    online: false,                                  // 设备在线状态
    props: {                                        // 云端物模型属性值
      LockState: 1,
      AuthType: 0,
      AuthResult: false,
      FaceCount: 0
    },
    authTypeText: '--',                             // 最近认证方式文本
    lastAuthText: '暂无记录',                        // 最近认证结果描述
    lastAuthOk: null,                               // 最近认证结果(样式)
    logs: [],                                       // 门禁动态(本地累计)
    pwdInput: ''                                    // 密码输入临时值
  },

  // ========== 设备配置(OneNET: 产品vleFpcs6C0 设备face 与单片机端一致) ==========
  config: {
    auth_info: "version=2018-10-31&res=products%2FvleFpcs6C0%2Fdevices%2Fface&et=2108271184&method=md5&sign=f54aIEYO9xxrk5ll54J8JA%3D%3D",
    product_id: "vleFpcs6C0",
    device_name: "face",
    api_base_url: "https://iot-api.heclouds.com"
  },
  // ========== 配置结束 ==========

  onLoad() {
    this.lastSnap = null                           // 属性快照(变化检测生成动态)
    this.fetchProps(true)
    this.startTimer()
  },

  onShow() {
    this.fetchProps(false)
  },

  onUnload() {
    this.timer && clearInterval(this.timer)
  },

  // 3秒定时轮询物模型属性(设备离线时降低为15秒)
  startTimer() {
    if (this.timer) clearInterval(this.timer)
    this.timer = setInterval(() => {
      this.fetchProps(false)
    }, 3000)
  },

  // 拉取OneNET物模型属性: 按identifier解析(不依赖数组顺序)
  fetchProps(isInit) {
    const { api_base_url, product_id, device_name, auth_info } = this.config
    wx.request({
      url: `${api_base_url}/thingmodel/query-device-property?product_id=${product_id}&device_name=${device_name}`,
      method: 'GET',
      header: { 'Authorization': auth_info },
      success: (res) => {
        if (res.data && res.data.code === 0 && Array.isArray(res.data.data)) {
          const props = {}
          res.data.data.forEach(item => {
            if (item.identifier === 'LockState') props.LockState = item.value
            else if (item.identifier === 'AuthType') props.AuthType = item.value
            else if (item.identifier === 'AuthResult') props.AuthResult = item.value
            else if (item.identifier === 'FaceCount') props.FaceCount = item.value
          })

          const authTypeText = AUTH_TYPE_MAP[props.AuthType] || '--'
          const lastAuthOk = props.AuthType ? !!props.AuthResult : null
          const lastAuthText = props.AuthType
            ? `${AUTH_TYPE_MAP[props.AuthType]}${props.AuthResult ? '成功' : '失败'}`
            : '暂无记录'

          this.setData({
            online: true,
            props,
            authTypeText,
            lastAuthOk,
            lastAuthText
          })

          // 非首次: 与快照对比, 有变化则生成本地门禁动态
          if (!isInit) this.detectChanges(props)
          this.lastSnap = { ...props }
        } else {
          this.setData({ online: false })
        }
      },
      fail: () => {
        this.setData({ online: false })
      }
    })
  },

  // 属性变化检测 -> 生成本地动态日志
  detectChanges(props) {
    const last = this.lastSnap
    if (!last) return
    const newLogs = []
    const time = util.formatTime(new Date())

    // 认证事件(认证方式或结果变化)
    if (props.AuthType !== last.AuthType || props.AuthResult !== last.AuthResult) {
      if (props.AuthType) {
        newLogs.push({
          time,
          type: props.AuthResult ? 'ok' : 'fail',
          text: `${AUTH_TYPE_MAP[props.AuthType] || '未知方式'}${props.AuthResult ? '认证成功' : '认证失败'}`
        })
      }
    }
    // 门锁状态变化
    if (props.LockState !== last.LockState) {
      newLogs.push({
        time,
        type: props.LockState === 0 ? 'ok' : 'info',
        text: props.LockState === 0 ? '仓库大门已开启' : '仓库大门已关闭'
      })
    }
    // 人脸库数量变化
    if (props.FaceCount !== last.FaceCount) {
      newLogs.push({
        time,
        type: 'info',
        text: `人脸库数量更新为 ${props.FaceCount} 人`
      })
    }

    if (newLogs.length > 0) {
      const logs = [...newLogs.reverse(), ...this.data.logs].slice(0, 30)
      this.setData({ logs })
    }
  },

  // 统一下发: OneNET thingmodel/set-device-property
  setProperty(params, done) {
    const { api_base_url, product_id, device_name, auth_info } = this.config
    wx.showLoading({ title: '指令下发中', mask: true })
    wx.request({
      url: `${api_base_url}/thingmodel/set-device-property`,
      method: 'POST',
      header: {
        'Authorization': auth_info,
        'Content-Type': 'application/json'
      },
      data: {
        product_id: product_id,
        device_name: device_name,
        params: params
      },
      success: (res) => {
        wx.hideLoading()
        if (res.data && res.data.code === 0) {
          wx.showToast({ title: '下发成功', icon: 'success', duration: 1500 })
          // 延时回读物模型, 尽快刷新单片机执行后的状态
          setTimeout(() => this.fetchProps(false), 2500)
        } else {
          wx.showToast({
            title: `下发失败:${res.data.msg || '平台拒绝'}`,
            icon: 'none',
            duration: 2500
          })
        }
      },
      fail: () => {
        wx.hideLoading()
        wx.showToast({ title: '网络错误', icon: 'none' })
      },
      complete: () => {
        done && done()
      }
    })
  },

  // 远程开门(单片机收到RemoteUnlock后开门+蜂鸣+上报AuthType=4)
  onRemoteUnlock() {
    wx.showModal({
      title: '远程开门确认',
      content: '确定要远程开启仓库大门吗？',
      confirmColor: '#1b6cff',
      success: (res) => {
        if (res.confirm) this.setProperty({ RemoteUnlock: true })
      }
    })
  },

  // 清除人脸库(单片机收到Face_Clear后向人脸模块发送删除全部指令)
  onFaceClear() {
    wx.showModal({
      title: '清除人脸库',
      content: '将删除设备端所有人脸信息, 此操作不可恢复, 确定继续吗？',
      confirmColor: '#e64340',
      success: (res) => {
        if (res.confirm) this.setProperty({ Face_Clear: true })
      }
    })
  },

  // 密码输入
  onPwdInput(e) {
    this.setData({ pwdInput: e.detail.value })
  },

  // 下发密码(单片机收到Password_Set后校验6位数字并写入EEPROM)
  onSetPassword() {
    const pwd = (this.data.pwdInput || '').trim()
    if (!/^\d{6}$/.test(pwd)) {
      wx.showToast({ title: '请输入6位数字密码', icon: 'none' })
      return
    }
    this.setProperty({ Password_Set: pwd }, () => {
      this.setData({ pwdInput: '' })
    })
  }
})
