from pymxs import runtime as rt


def normal_to_world(normal, node, epsilon=1.0e-8):
    """
    メッシュローカル法線をワールド座標系へ変換する。

    objectTransformを使用するため、以下をまとめて考慮する。
    - ノードの移動・回転・スケール
    - 親階層
    - Object Offset
    - 非均一スケール
    - 1軸、2軸、3軸の負スケール
    - シアー

    Parameters
    ----------
    normal : pymxs.runtime.Point3
        メッシュローカル座標系の法線。
    node : pymxs.runtime.MAXWrapper
        法線が属する3ds Maxノード。
    epsilon : float
        特異行列およびゼロ法線の判定値。

    Returns
    -------
    pymxs.runtime.Point3
        正規化されたワールド座標系の法線。

    Raises
    ------
    ValueError
        0スケールなどでobjectTransformが逆行列を持たない場合。
    """
    tm = node.objectTransform

    determinant = rt.dot(
        rt.cross(tm.row1, tm.row2),
        tm.row3,
    )

    if abs(float(determinant)) <= epsilon:
        raise ValueError(
            "objectTransformが特異行列です。"
            "いずれかの軸のスケールが0である可能性があります。"
        )

    inverse_tm = rt.inverse(tm)

    # normal * transpose(inverse(objectTransform))
    # pymxs/MAXScriptは行ベクトル形式なので、各成分を内積で計算する。
    world_normal = rt.point3(
        rt.dot(normal, inverse_tm.row1),
        rt.dot(normal, inverse_tm.row2),
        rt.dot(normal, inverse_tm.row3),
    )

    normal_length = float(rt.length(world_normal))

    if normal_length <= epsilon:
        raise ValueError("変換後の法線がゼロベクトルです。")

    return world_normal / normal_length