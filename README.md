# AndroidProjectLibraries

アンドロイドアプリを開発するために個人的に作ったライブラリです。2022/9/8現在以下のリポジトリのsubmoduleとして用いられています。

- https://github.com/reika727/Salmon
- https://github.com/reika727/SlitCamera

## おーばーびゅー

### image_utility

高速で画像処理を行うためにC++で実装したライブラリです。

<dl>
  <dt>image_accessor_YUV_420_888</dt>
  <dd>カメラから受け取ったImageオブジェクトのピクセルへアクセスするためのクラスです。</dd>
  <dt>surface_texture_accessor_R8G8B8X8</dt>
  <dd>SurfaceTextureのピクセルへアクセスするためのクラスです。</dd>
  <dt>coordinate_transformer</dt>
  <dd>Imageのピクセル座標をSurfaceTexture上で対応する座標に変換します。</dd>
  <dt>YUV_to_RGB</dt>
  <dd>YUVをRGBに変換します。</dd>
</dl>
