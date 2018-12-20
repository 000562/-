var app = getApp()


Page({
  data: {
    height: 0
  },
  onLoad() {
    this.setData({
      height: app.globalData.temperature.datapoints[0].value
    })
  },
  onPullDownRefresh: function () {
    console.log('onPullDownRefresh', new Date())
  }

})