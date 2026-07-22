case EMoviePipelineObjectIdPassIdType::CustomStencil:
{
	const int32 StencilValue =
		InPrimComponent->GetCustomDepthStencilValue();

	if (StencilValue > 0)
	{
		StringBuilder.Appendf(
			TEXT("Stencil_%03d"),
			StencilValue
		);
	}

	break;
}